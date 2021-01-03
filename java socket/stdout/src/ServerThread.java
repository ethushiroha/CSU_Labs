import java.io.PrintWriter;
import java.util.ArrayList;

public class ServerThread extends Thread {
    private User user;
    private ArrayList<User> userList;
    public boolean exited;


    public ServerThread(User user, ArrayList<User> userList) {
        this.user = user;
        this.userList = userList;
        this.exited = false;

    }

    @Override
    public void run() {
        try {
            while (!this.exited) {
                PrintWriter pw = user.getPw();
                pw.print("Commands: ");
                pw.flush();

                String msg = user.getBr().readLine();
                System.out.println(msg);

                this.dealMsg(msg);
            }
        }
        catch (Exception e) {
            System.err.println("error!");
        }
        finally {
            try {
                user.getBr().close();
                user.getSocket().close();
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void removeUser(User user) {
        this.userList.remove(user);
        System.out.println(user.getName() + " is logout");
    }

    private void sendMsg(String userName, String msg) {
        for (User user1 : this.userList) {
            if ((user1.getName().equals(userName)) || (userName.equals("Server"))) {
                try {
                    PrintWriter pw = user1.getPw();

                    String message;
                    if (userName.equals("Server")) {
                        message = "Server says: " + msg;
                    }else {
                        message = user.getName() + " says: " + msg;
                    }
                    pw.println(message);
                    pw.flush();
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void sendMsg(String fromUser, String groupName, String msg) {
        for (User user1 : this.userList) {
            if (user1.getGroup().equals(groupName)) {
                String title = "group message: " + fromUser + " says: " + msg;
                try {
                    PrintWriter pw = user1.getPw();

                    pw.println(title);
                    pw.flush();
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void sendLogout() {
        PrintWriter pw = this.user.getPw();
        pw.println("logout~");
        pw.flush();
    }

    private void helpCase() {
        String msg = "\n";
        msg += "/join,(group_name): join to a group\n";
        msg += "/whoami: show your name and group\n";
        msg += "/group,(something): say something to the group\n";
        msg += "/say,(username),(something) say something to a person\n";

        sendMsg("Server", msg);
    }

    private void dealMsg(String msg) {
        String[] str = msg.split(",");
        switch (str[0]) {
            case "/group":
                sendMsg(user.getName(), user.getGroup(), str[1]);
                break;
            case "/help":
                helpCase();
                break;
            case "/whoami":
                sendMsg("Server", "your name is: " + user.getName());
                sendMsg("Server", "your group is: " + user.getGroup());
                break;
            case "/join":
                user.setGroup(str[1]);
                sendMsg("Server", " join group " + str[1] + " success!");
                break;
            case "/say":
                // send to Other;
                sendMsg(str[1], msg);
                break;
            case "/logout":
                // logout
                sendLogout();
                removeUser(this.user);
                this.exited = true;
                break;
            default:
                // tell user input error;
                sendMsg("Server", "error commands");
                break;
        }
    }
}
