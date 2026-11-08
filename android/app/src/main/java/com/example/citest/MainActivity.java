package com.example.citest;

import android.os.Bundle;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView version = findViewById(R.id.version);
        version.setText(getString(R.string.version_text, Version.parse(BuildConfig.VERSION_NAME).toString()));

        TextView appId = findViewById(R.id.app_id);
        appId.setText(getPackageName());
    }
}
