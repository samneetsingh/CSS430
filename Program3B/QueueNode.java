/*
 * This is a template of QueueNode.java. Change this file name into QueueNode.java
 * and implement the logic.
 */

import java.util.*;

public class QueueNode {
    private Vector<Integer> tidQueue; // maintains a list of child tidQueue who called wakeup( ).

    public QueueNode( ) {
        tidQueue = new Vector<Integer>();
    }

    public synchronized int sleep( ) {
        if (tidQueue.size() == 0) {
            try {
                wait();
            }
            catch (InterruptedException e){}
            return tidQueue.remove(0);
        }

        return -1;
    }

    public synchronized void wakeup( int tid ) {
    // TODO: implement this method
	// Add this child TID to tidQueue.
	// Notify the parent.
        tidQueue.add(tid);
        notify();
    }
}
