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
public class Notification {
    
    enum Type {
        UNKNOW(-1),
        ValueAdded(0),
        ValueRemoved(1),
        ValueChanged(2),
        ValueRefreshed(3),
        Group(4),
        NodeNew(5),
        NodeAdded(6),
        NodeRemoved(7),
        NodeProtocolInfo(8),
        NodeNaming(9),
        NodeEvent(10),
        PollingDisabled(11),
        PollingEnabled(12),
        SceneEvent(13),
        CreateButton(14),
        DeleteButton(15),
        ButtonOn(16),
        ButtonOff(17),
        DriverReady(18),
        DriverFailed(19),
        DriverReset(20),
        EssentialNodeQueriesComplete(21),
        NodeQueriesComplete(22),
        AwakeNodesQueried(23),
        AllNodesQueriedSomeDead(24),
        AllNodesQueried(25),
        Notification(26),
        DriverRemoved(27),
        ControllerCommand(28),
        NodeReset(29);

        private final int id;
        Type(int id) {
            this.id = id;
        }

        private static Type forId(int id){
            for (Type type : values()) {
                if(type.id == id){
                    return type;
                }
            }
            return UNKNOW;
        }
    }
    
    private Type type;
    private long homeId;
    private short nodeId;
    private ValueId valueId;
    private short groupIdx;
    private short event;
    private short buttonId;
    private short sceneId;
    private short notification;
    private short bytes;
    
    
    public Notification(int type, long homeId, short nodeId, ValueId valueId, short groupIdx, short event, short buttonId, short sceneId, short notification, short bytes){
        this.type = Type.forId(type);
        this.homeId = homeId;
        this.nodeId = nodeId;
        this.valueId = valueId;
        this.groupIdx = groupIdx;
        this.event = event;
        this.buttonId = buttonId;
        this.sceneId = sceneId;
        this.notification = notification;
        this.bytes = bytes;
    }

    public Type getType() {
        return type;
    }

    public long getHomeId() {
        return homeId;
    }

    public short getNodeId() {
        return nodeId;
    }

    public ValueId getValueId() {
        return valueId;
    }

    public short getGroupIdx() {
        return groupIdx;
    }

    public short getEvent() {
        return event;
    }

    public short getButtonId() {
        return buttonId;
    }

    public short getSceneId() {
        return sceneId;
    }

    public short getNotification() {
        return notification;
    }

    public short getBytes() {
        return bytes;
    }

    @Override
    public String toString() {
        return "Notification{" +
                "type=" + type +
                ", homeId=" + homeId +
                ", nodeId=" + nodeId +
                ", valueId=" + valueId +
                ", groupIdx=" + groupIdx +
                ", event=" + event +
                ", buttonId=" + buttonId +
                ", sceneId=" + sceneId +
                ", notification=" + notification +
                ", bytes=" + bytes +
                '}';
    }
}
