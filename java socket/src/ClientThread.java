import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;

public class ClientThread extends Thread {
    private Socket socket;
    private boolean exited;

    public ClientThread(Socket socket) {
        this.socket = socket;
        this.exited = false;
    }

    public void run() {
        try {
            InputStream inputStream = socket.getInputStream();
            InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
            BufferedReader br = new BufferedReader(inputStreamReader);
            try {
                while (!this.exited) {
                    String msg = br.readLine();
                    if (msg.equals("logout~")) {
                        socket.close();
                        this.exited = true;
                        continue;
                    }
                    System.out.println(msg);

                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
