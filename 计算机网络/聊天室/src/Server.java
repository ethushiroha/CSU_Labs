import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class Server {
    private static boolean Judge(String name, String pwd) {
        if (name.equals("stdin") && pwd.equals("123")) {
            return true;
        } else if (name.equals("stdout") && pwd.equals("123")){
            return true;
        }
        return false;
    }

    private static String Login(Socket socket) throws IOException {
        String ret;
        BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        String st = br.readLine();
        String[] str = st.split(":");
        String msg;
        // 验证用户名密码
        if (Judge(str[0], str[1])) {
            msg = "success";
            ret = str[0];
        } else {
            msg = "failed";
            ret = null;
        }
        PrintWriter pw = new PrintWriter(socket.getOutputStream());
        pw.println(msg);
        pw.flush();

        return ret;

    }

    public static void startServer() throws IOException {
        ArrayList<User> userList = new ArrayList<User>();
        ServerSocket serverSocket= new ServerSocket(8848);
        System.out.println("listening....");
        while (true) {
            Socket socket = serverSocket.accept();
            String name = Login(socket);
            if (name.equals(null)) {
                socket.close();
            } else  {
                User user = new User(name, socket);
                userList.add(user);
                ServerThread serverThread = new ServerThread(user, userList);
                serverThread.start();
            }
        }

    }

    public static void main(String[] args) throws IOException {
        try {
            startServer();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

}
