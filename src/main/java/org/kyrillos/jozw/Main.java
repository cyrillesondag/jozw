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
public class Main {
    
    public static void main(String[] agrs) throws InterruptedException {
        Ozw ozw = Ozw.getInstance();
        
        System.out.println(Ozw.getVersionAsString());
        ozw.addDriver("/dev/ttyACM0", Ozw.ControllerInterface.SERIAL);

        //Thread.sleep(60000);
    }
}
