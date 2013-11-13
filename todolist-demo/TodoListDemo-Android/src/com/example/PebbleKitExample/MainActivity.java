package com.example.PebbleKitExample;

import android.R;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import com.example.PebbleKitExample.TodoList.ExampleTodoListActivity;

public class MainActivity extends ListActivity {

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String[] menu = {
                "Sports App Example",
                "Golf App Example",
                "Weather App Example",
                "To-Do List Example",
                "GPS/SMS App Example",
                "GCM Activity"
        };

        setListAdapter(new ArrayAdapter<String>(this, R.layout.simple_list_item_1, menu));
    }

    @Override
    protected void onListItemClick(final ListView l, final View v, final int position, final long id) {
        Intent intent = null;
        switch (position) {
            case 0:
                intent = new Intent(this, ExampleSportsActivity.class);
                break;
            case 1:
                intent = new Intent(this, ExampleGolfActivity.class);
                break;
            case 2:
                intent = new Intent(this, ExampleWeatherActivity.class);
                break;
            case 3:
                intent = new Intent(this, ExampleTodoListActivity.class);
                break;
            case 4:
                intent = new Intent(this, ExampleSmsActivity.class);
                break;
            case 5:
                intent = new Intent(this, ExampleGCMActivity.class);
                break;
        }

        if (intent != null) {
            startActivity(intent);
        }
    }
}
