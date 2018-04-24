package org.kyrillos.jozw;

public class InstanceAssociation {

    private final int nodeId;
    private final int instanceId;

    public InstanceAssociation(int nodeId, int instanceId) {
        this.nodeId = nodeId;
        this.instanceId = instanceId;
    }

    public int nodeId() {
        return nodeId;
    }

    public int instanceId() {
        return instanceId;
    }
}
