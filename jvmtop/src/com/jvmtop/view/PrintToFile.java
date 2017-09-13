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

import java.io.File;
import java.io.FileWriter;

/**
 * TODO: document this type!
 *
 * @author youhan
 *
 */
public class PrintToFile {
	/** Logger to be used in this class */
	  private static String oPath;
	  public static File output; public File getOutput(){return output;}
	  public static FileWriter fw; public FileWriter getFW(){return fw;}
	  public PrintToFile(String _oPath, File _output, FileWriter _fw){
		  oPath=_oPath;
		  fw=_fw;
		  output=_output;
	  }
}
