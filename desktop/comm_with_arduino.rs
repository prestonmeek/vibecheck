// REQUIRED: "cargo add serialport"
use serialport::SerialPortType::UsbPort;

use std::time;
use time::{ SystemTime, UNIX_EPOCH };

use std::io;
use io::Write;

use std::thread;

fn main() {
    println!("Scanning ports...");

    // continuously scan all the ports for an arduino
    loop {
        // go through each serial device
        for port_info in serialport::available_ports().unwrap() {
            // if is a usb device
            if let UsbPort(usb_port_info) = port_info.port_type {
                // check if is arduino (VID of arduino llc is 9025)
                if usb_port_info.vid == 9025 {
                    println!("Arduino found on {:?}. Attempting to open port...", port_info.port_name);
                    
                    // attempt to open as a regular port
                    if let Ok(mut port) = serialport::new(port_info.port_name, 115200).open() {
                        println!("Port opened successfully");

                        // get system time
                        let utc_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_secs();
                        println!("Relaying current time ({:?})", utc_time);

                        // write system time to port
                        port.write(utc_time.to_string().as_bytes()).expect("Could not write time");
                        
                        // finally, just re-print the incoming serial data
                        println!("Printing serial data:");
                        let mut serial_buf: Vec<u8> = vec![0; 1];
                        while let Ok(_) = port.read(serial_buf.as_mut_slice()) {
                            io::stdout().write_all(&serial_buf).unwrap();
                        }

                        // if we get here, the connection has been terminated (no data has been sent)
                        println!("\nConnection lost. Attempting to reconnect...");
                        break;
                    } else {
                        println!("Could not open port");
                    }
                }
            }
        }

        // wait 100ms in between each scan
        thread::sleep(time::Duration::from_millis(100));
    }
}
