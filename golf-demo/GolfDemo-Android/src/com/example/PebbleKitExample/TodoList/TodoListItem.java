package com.example.PebbleKitExample.TodoList;

public class TodoListItem {
    private boolean mIsComplete;
    private String mDescription;


    private TodoListItem(final boolean isComplete, final String description) {
        mIsComplete = isComplete;
        mDescription = description;
    }

    public static TodoListItem getItemWithDescription(final String description) {
        return new TodoListItem(false, description);
    }


    public boolean isComplete() {
        return mIsComplete;
    }

    public void setComplete(final boolean isComplete) {
        mIsComplete = isComplete;
    }

    public String getDescription() {
        return mDescription;
    }

    public void setDescription(final String description) {
        mDescription = description;
    }

    @Override
    public String toString() {
        return mDescription;
    }
}
