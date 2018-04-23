/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package org.kyrillos.jozw;

/**
 *
 * @author cyrille
 */
public class ValueId {

    private long nativePtr;
    private long homeId;
    private short nodeId;
    private int genre;
    private short commandClassId;
    private short instance;
    private short index;
    private int type;
    
    public ValueId(long homeId, short nodeId, int genre, short commandClassId, short instance, short index, int type){
        this.homeId = homeId;
        this.nodeId = nodeId;
        this.genre = genre;
        this.commandClassId = commandClassId;
        this.instance = instance;
        this.index = index;
        this.type = type;
    }

    public long getHomeId() {
        return homeId;
    }

    public short getNodeId() {
        return nodeId;
    }

    public int getGenre() {
        return genre;
    }

    public short getCommandClassId() {
        return commandClassId;
    }

    public short getInstance() {
        return instance;
    }

    public short getIndex() {
        return index;
    }

    public int getType() {
        return type;
    }

    @Override
    public String toString() {
        return "ValueId{" +
                "homeId=" + homeId +
                ", nodeId=" + nodeId +
                ", genre=" + genre +
                ", commandClassId=" + commandClassId +
                ", instance=" + instance +
                ", index=" + index +
                ", type=" + type +
                '}';
    }
}
