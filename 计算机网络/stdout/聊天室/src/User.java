import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;

public class User {
    private String name;

    private Socket socket;
    public ArrayList<String> friends;
    private PrintWriter pw;
    private BufferedReader br;
    public ArrayList<String> group;

    public User(String name, Socket socket) throws IOException {
        this.name = name;
        this.socket = socket;
        this.friends = new ArrayList<String>();
        // 测试数据
        this.friends.add("Server");
        this.group = new ArrayList<String>();
        // 测试数据
        this.group.add("all");
        this.pw = new PrintWriter(socket.getOutputStream());
        this.br = new BufferedReader(new InputStreamReader(socket.getInputStream()));

    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Socket getSocket() {
        return socket;
    }

    public void setSocket(Socket socket) {
        this.socket = socket;
    }

    public PrintWriter getPw() {
        return pw;
    }

    public void setPw(PrintWriter pw) {
        this.pw = pw;
    }

    public BufferedReader getBr() {
        return br;
    }

    public void setBr(BufferedReader br) {
        this.br = br;
    }

    public ArrayList<String> getFriends() {
        return friends;
    }

    public void setFriends(ArrayList<String> friends) {
        this.friends = friends;
    }

    public ArrayList<String> getGroup() {
        return group;
    }

    public void setGroup(ArrayList<String> group) {
        this.group = group;
    }
}
