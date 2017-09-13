/**
 * jvmtop - java monitoring for the command-line
 * 
 * Copyright (C) 2017 by Patric Rufflar. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
package com.jvmtop.view;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Dialog.ModalityType;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

import com.jvmtop.JvmTop;

/**
 * TODO: document this type!
 *
 * @author youhan
 *
 */
public class FrequencySet extends JDialog {

    /**
     * 
     */
	private JLabel label=new JLabel();
    private JTextField field=new JTextField();
    
    private JButton button = new JButton("Confirm");
    JvmTop jt;
    public FrequencySet(JvmTop jt){
        label.setText("Update Frequency(s)"); 
        field.setText("1.0");
        this.jt=jt;
        label.setSize(150, 30);
        field.setSize(80, 30);
        button.setSize(80,40);
        
        button.addActionListener(new ActionListener() {
            
            @Override
            public void actionPerformed(ActionEvent e) {
            	//int tmp=Integer.parseInt(field.getText());
            	ButtonActionPerformed(e);
            }
        });
        
        this.add(label);
        this.add(field);
        this.add(button);
        this.setLocation(4008,400);
        this.setSize(420, 100);
        this.setModal(true);
    }
    private void ButtonActionPerformed(ActionEvent evt)
    {
  	  	double tmp=Double.parseDouble(field.getText());
  	  	jt.setDelay(tmp);
  	  	this.dispose();
    }
}