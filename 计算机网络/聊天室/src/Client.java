import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

public class Client {
    public Socket connects(String ip, int port) throws IOException {
        Socket socket = new Socket(ip, port);
        return socket;
    }

    public void sendMsg(Socket socket, String msg) throws IOException {
        PrintWriter pw = new PrintWriter(socket.getOutputStream());
        pw.println(msg);
        pw.flush();
    }
}
