import java.io.*;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Semaphore;

public class OrderTask extends Thread {

    public static final Object lock = new Object();

    private static final String ORDER_OUTPUT_FILE = "orders_out.txt";
    private static final String ORDERS_FILE = "orders.txt";

    private final LineNumberReader reader;
    private final Semaphore semaphore;
    private final String path;
    private Integer lineNo;
    private final ConcurrentHashMap<String, Integer> localOrders = new ConcurrentHashMap<>();
    private final Integer step;

    private static PrintWriter printWriter = null;

    public OrderTask(String path, Integer lineNo, Integer step, Semaphore productSemaphore)
            throws Exception {
        this.lineNo = lineNo;
        this.step = step;
        this.semaphore = productSemaphore;

        String currentDirectory = Paths.get("").toAbsolutePath().toString();
        String inputFolder=currentDirectory+"/"+path;

        String ordersFile = inputFolder + "/" + ORDERS_FILE;
        String outputOrdersFile = currentDirectory + "/" + ORDER_OUTPUT_FILE;
        this.reader = new LineNumberReader(new FileReader(ordersFile));
        this.path = path;

        if (printWriter == null) {
            synchronized (lock) {
                try {
                    FileWriter fileWriter = new FileWriter(outputOrdersFile);
                    printWriter = new PrintWriter(fileWriter);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    @Override
    public void run() {
        boolean flag = true;
        while (flag) {
            String line;
            try {
                line = reader.readLine();
                if (line != null) {
                    if (lineNo == reader.getLineNumber()) {
                        String[] split = line.split(",");
                        String orderId = split[0];
                        int noOfProducts = Integer.parseInt(split[1]);
                        localOrders.put(orderId, noOfProducts);
                        lineNo += step;
                    }
                } else {
                    flag = false;
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
        /**
         * Pornirea procesarii comenzilor.
         * Pentru a evita monopolizarea thread-urilor de nivel 2 vom crea alte 1/2 * P thread-uri
         * si ne vom utiliza de un semafor
         */
        Enumeration<String> orders = localOrders.keys();
        while (orders.hasMoreElements()) {
            List<ProductTask> productTasks = new ArrayList<>();
            String orderid = orders.nextElement();
            Integer noOfProducts = localOrders.get(orderid);
            for (int i = 0; i < noOfProducts; i++) {
                ProductTask task = new ProductTask(semaphore, orderid, i + 1, path);
                task.start();
                productTasks.add(task);
            }
            for (ProductTask productTask : productTasks) {
                try {
                    productTask.join();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
            /**
             * Toate thread-urile au ajuns la sfarsit.
             * Scriem comanda
             */
            try {
                if(noOfProducts != 0){
                writeOrder(orderid, noOfProducts);
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    /**
     * Scriem un rand dupa formatul o_zoqiqpw2ly,shipped
     *
     * @param orderId     comanda pt care am trimis produsul
     * @param noOfProducts numarul de produse dintr-o comanda
     */
    public void writeOrder(String orderId, Integer noOfProducts) throws IOException {
        synchronized (lock) {
            printWriter.println(String.format("%s,%d,shipped", orderId, noOfProducts));
        }
    }

    /**
     * Facem flush inainte de a inchide fisierul pentru a salva toata informatia
     */
    public static void closeFile() {
        synchronized (lock) {
            printWriter.flush();
            printWriter.close();
        }
    }
}
