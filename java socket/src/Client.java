import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class Client {
    public static void startCient(HashMap<String, String> hashMap) {
        String ip = "127.0.0.1";
        int port = 8848;
        try {
            Socket socket = new Socket(ip, port);
            PrintWriter pw1 = new PrintWriter(socket.getOutputStream());
            BufferedReader bf1 = new BufferedReader(new InputStreamReader(socket.getInputStream()));

//            System.out.print("name: ");
//            Scanner sc = new Scanner(System.in);
//            String name = sc.next();
//            pw1.println(name);
//            pw1.flush();
            pw1.println(hashMap.toString());
            pw1.flush();

            System.out.println(bf1.readLine());

            ClientThread clientThread = new ClientThread(socket);
            clientThread.setName("User1");
            clientThread.start();

            BufferedReader bf = new BufferedReader(new InputStreamReader(System.in));
            PrintWriter pw = new PrintWriter(socket.getOutputStream());
            try {
                while (true) {
                    String st = bf.readLine();
                    pw.println(st);
                    pw.flush();
                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
            finally {
                socket.close();
                pw.close();
                bf.close();
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        HashMap<String, String> hashMap = new HashMap<String, String>();
        hashMap.put("shiroha", "123");
        startCient(hashMap);
    }

}
