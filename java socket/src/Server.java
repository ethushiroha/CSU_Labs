import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class Server {
    public static void startServer() throws IOException{
        ArrayList<User> userList = new ArrayList<User>();

        ServerSocket serverSocket = new ServerSocket(8848);
        System.out.println("start listening...");

        int i = 0;
        while(true) {
            Socket socket = serverSocket.accept();

            User user = login(socket);

            System.out.println("User: " + user.getName() + " is loading...");

            userList.add(user);

            ServerThread serverThread = new ServerThread(user, userList);

            serverThread.start();

            BufferedReader bf1 = new BufferedReader(new InputStreamReader(System.in));

            try {
                while (true) {
                    String st = bf1.readLine();
                    String[] str = st.split(",");
                    if (str[0].equals("/all")) {
                        sendToAll(userList, str[1]);
                    }
                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] args) throws IOException {
        startServer();
    }

    public static User login(Socket socket) throws IOException {
        PrintWriter pw = new PrintWriter(socket.getOutputStream());
        BufferedReader bf = new BufferedReader(new InputStreamReader(socket.getInputStream()));

        pw.println("who are you: ");
        pw.flush();

        String userpass = bf.readLine();
        String[] name;

        name = userpass.split("=");
        int position = userpass.indexOf("=");
        name[0] = name[0].substring(1);
        name[1] = name[1].substring(0, name[1].length() - 1);

//        if (!(name[0].equals("shiroha")) || !(name[1].equals("123"))) {
//            pw.println("login failed");
//            pw.flush();
//            socket.close();
//        }
        pw.println("Server says: welcome " + name[0]);
        pw.flush();

        User user = new User(name[0], socket);
        return user;

    }

    public static void sendToAll(List<User> userList, String msg) {
        String st = "Announce to all: " + msg;
        for (User user : userList) {
            PrintWriter pw = user.getPw();
            pw.println(st);
            pw.flush();
        }
    }

}

