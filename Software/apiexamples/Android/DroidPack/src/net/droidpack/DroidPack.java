package net.droidpack;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class DroidPack extends Activity {

    private static String TAG = "DroidPack";
    private Socket sock = null;

    Button btnConnect;
    Button btnDisconnect;
    Button btnLockUnlock;
    Button btnR;
    Button btnG;
    Button btnB;
    boolean locked = false;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        btnConnect = (Button) findViewById(R.id.btnConnect);
        btnDisconnect = (Button) findViewById(R.id.btnDisconnect);
        btnLockUnlock = (Button) findViewById(R.id.btnLockUnlock);
        btnR = (Button) findViewById(R.id.btnRed);
        btnG = (Button) findViewById(R.id.btnGreen);
        btnB = (Button) findViewById(R.id.btnBlue);
        btnConnect.setOnClickListener(new OnClickListener() {

            public void onClick(View view) {
                EditText editIp = (EditText) findViewById(R.id.editIp);
                EditText editPort = (EditText) findViewById(R.id.editPort);
                try {
                    sock = new Socket();
                    final String ip = editIp.getText().toString();
                    final int port = Integer.parseInt(editPort.getText().toString());
                    Log.i(TAG, String.format("ip: %s; port: %s", ip, String.valueOf(port)));
                    sock.connect(new InetSocketAddress(ip, port));
                    Log.i(TAG, "socket has been created");
                    onConnectedChanged(true);
                } catch (UnknownHostException ex) {
                    Log.e(TAG, ex.getMessage());
                } catch (IOException ex) {
                    Log.e(TAG, ex.getMessage());
                }
            }

        });
        
        btnDisconnect.setOnClickListener(new OnClickListener() {

            public void onClick(View view) {
                try {
                    sock.close();
                    sock = null;
                    Log.i(TAG, "socket has been disconnected");
                    onConnectedChanged(false);
                } catch (UnknownHostException ex) {
                    Log.e(TAG, ex.getMessage());
                } catch (IOException ex) {
                    Log.e(TAG, ex.getMessage());
                }
            }
        });

        btnLockUnlock.setOnClickListener(new OnClickListener() {

            public void onClick(View view) {
                if (!locked) {
                    try {
                        sock.getOutputStream().write("lock\n".getBytes());
                        locked = true;
                        btnLockUnlock.setText("Unlock");
                    } catch (IOException ex) {
                        Log.e(TAG, ex.getMessage());
                    }
                } else {
                    try {
                        sock.getOutputStream().write("unlock\n".getBytes());
                        locked = false;
                        btnLockUnlock.setText("Lock");
                    } catch (IOException ex) {
                        Log.e(TAG, ex.getMessage());
                    }
                }
                try {
                    sock.getInputStream().read();
                } catch (IOException ex) {
                    Log.e(TAG, ex.getMessage());
                }
            }
        });

        Button btnRed = (Button) findViewById(R.id.btnRed);
        btnRed.setOnClickListener(new OnClickListener() {

            public void onClick(View view) {
                setAll(sock, 255, 0, 0);
            }
        });
        
        Button btnGreen = (Button) findViewById(R.id.btnGreen);
        btnGreen.setOnClickListener(new OnClickListener() {

            public void onClick(View view) {
                setAll(sock, 0, 255, 0);
            }
        });

        Button btnBlue = (Button) findViewById(R.id.btnBlue);
        btnBlue.setOnClickListener(new OnClickListener() {

            public void onClick(View view) {
                setAll(sock, 0, 0, 255);
            }
        });
    }
    
    private void onConnectedChanged(boolean isConnected) {
        btnConnect.setEnabled(!isConnected);
        btnDisconnect.setEnabled(isConnected);
        btnLockUnlock.setEnabled(isConnected);
        if (!isConnected) {
            locked = false;
            btnLockUnlock.setText("Lock");
        }
        btnR.setEnabled(isConnected);
        btnG.setEnabled(isConnected);
        btnB.setEnabled(isConnected);
    }

    private void setAll(Socket socket, int r, int g, int b) {
        for(int i = 1; i <= 10; i++) {
            try {
                sock.getOutputStream().write(String.format("setcolor:%s-%s,%s,%s\n", String.valueOf(i), String.valueOf(r), String.valueOf(g), String.valueOf(b)).getBytes());
            } catch (IOException ex) {
                Log.e(TAG, ex.getMessage());
            }
        }
    }
}
