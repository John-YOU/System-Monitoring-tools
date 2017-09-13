//this file is for testing jvmtop and iotop
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.File;

import java.util.concurrent.TimeUnit;

public class Cout {

	private static final String filename = "./data.txt";

	 public static void main(String[] args) throws InterruptedException,IOException {

         File f=new File(filename);
         f.createNewFile();

         try {
                 int i=1;
                 while (i>0)
                 {
                	 	BufferedWriter bw = new BufferedWriter(new FileWriter(filename,true));
                         TimeUnit.MILLISECONDS.sleep(100);
                         String content = "This is the content to write into file\n";
                         bw.write(content);
                         bw.close();
                 }


                 // no need to close it.
                 //bw.close();

                 System.out.println("Done");

         } catch (IOException e) {

                 e.printStackTrace();

         }

 }


}
