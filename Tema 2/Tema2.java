import java.nio.file.Paths;
import java.util.concurrent.Semaphore;

public class Tema2 {
    /**
     * Sample usage: java Tema2 <folder_input> <nr_max_threads>
     *
     * @param args numaele folderului si numarul maxim de thread-uri
     */
    public static void main(String[] args) throws Exception {
        /**
         * Citirea parametrilor programului
         */
        if(args==null || args.length!=2){
            System.out.println("Usage: java Tema2 <folder_input> <nr_max_threads>");
            System.exit(1);
        }
        long start = System.currentTimeMillis();
        String inputFolder = args[0];
        int maxThreads = Integer.parseInt(args[1]);

        //processing the order.txt file
        Thread[] workers = new Thread[maxThreads];
        Semaphore productSemaphore = new Semaphore(maxThreads);

        /**
         * Pornirea vectorului worker pentru procesarea comenzilor.
         * Foloim toate cele P thread-uri pentru citire.
         * Am folosit aceasi variabila P ca si pas in citirea comnezilor.
         */
        for (int i = 0; i < maxThreads; i++) {
            workers[i] = new OrderTask(inputFolder, i+1,maxThreads,productSemaphore);
            workers[i].start();
        }

        for (Thread worker : workers) {
            worker.join();
        }
        OrderTask.closeFile();
        ProductTask.closeFile();
    }
}