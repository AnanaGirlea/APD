import java.io.*;
import java.nio.file.Paths;
import java.util.Set;
import java.util.concurrent.Semaphore;
public class ProductTask extends Thread {
    public static final Object lock = new Object();
    private static final String ORDER_PRODUCT_OUTPUT_FILE = "order_products_out.txt";
    private static final String ORDER_PRODUCT_FILE = "order_products.txt";
    private final Semaphore semaphore;
    private final int prodNo;
    private final String productsFile;
    private static PrintWriter printWriter=null;
    private final String orderId;
    public ProductTask(Semaphore semaphore, String orderId, int prodNo, String path){
        this.semaphore = semaphore;
        this.orderId = orderId;
        this.prodNo = prodNo;

        String currentDirectory = Paths.get("").toAbsolutePath().toString();
        String inputFolder=currentDirectory+"/"+path;
        this.productsFile = inputFolder + "/" + ORDER_PRODUCT_FILE;
        String outputProductsFile = currentDirectory + "/" + ORDER_PRODUCT_OUTPUT_FILE;

        if(printWriter==null) {
            synchronized (lock) {
                try {
                    FileWriter fileWriter = new FileWriter(outputProductsFile);
                    printWriter = new PrintWriter(fileWriter);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    @Override
    public void run() {
        try {
            LineNumberReader reader = new LineNumberReader(new FileReader(productsFile));
            semaphore.acquire(1);
            int found = 0;
            String line = reader.readLine();
            String pid = null;
            while (found < prodNo && line != null) {

                String[] split = line.split(",");
                String oid = split[0];
                pid = split[1];
                if (oid.equals(orderId))
                    found++;

                line = reader.readLine();
            }
            reader.close();
            if (found == prodNo) {
                writeProduct(orderId, pid);
            }
            semaphore.release();
        } catch (IOException | InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Scriem un rand dupa formatul o_zoqiqpw2ly,p_w9x719iaco,shipped
     *
     * @param orderId   comanda pt care am trimis produsul
     * @param productId produs pe care l-am trimis
     */
    public void writeProduct(String orderId, String productId) throws IOException {
        synchronized (lock) {
            printWriter.println(String.format("%s,%s,shipped", orderId, productId));
        }
    }

    /**
     * Facem flush inainte de a inchide fisierul pentru a salva toata informatia
     */
    public static void closeFile()  {
        synchronized (lock) {
            printWriter.flush();
            printWriter.close();
        }
    }
}
