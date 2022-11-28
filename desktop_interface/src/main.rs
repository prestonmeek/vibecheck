use serialport::SerialPortType::UsbPort;

use std::time;
use time::{SystemTime, UNIX_EPOCH};

use io::Write;
use std::io;

use std::thread;

// time offset in seconds (-5 hours * 60 min/hr * 60 sec/min)
const TIME_OFFSET: i64 = -5 * 60 * 60;
const BAUD_RATE: u32 = 19200;

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
                    println!(
                        "Arduino found on {:?}. Attempting to open port...",
                        port_info.port_name
                    );

                    // attempt to open as a COM port
                    if let Ok(mut port) = serialport::new(port_info.port_name, BAUD_RATE).open() {
                        println!("Port opened successfully");

                        // get system time
                        let utc_time = SystemTime::now()
                            .duration_since(UNIX_EPOCH)
                            .unwrap()
                            .as_secs();
                        // get local time
                        let local_time = utc_time as i64 + TIME_OFFSET;

                        println!("Relaying current time ({:?})", local_time);

                        // write system time to port
                        port.write_all(local_time.to_string().as_bytes())
                            .expect("Could not write time");

                        // finally, just re-print the incoming serial data
                        println!("Printing serial data:");

                        let mut serial_buf: [u8; 1] = [0; 1];
                        while port.read(serial_buf.as_mut_slice()).is_ok() {
                            let to_write = if std::str::from_utf8(&serial_buf).is_ok() {
                                serial_buf
                            } else {
                                [b'?'; 1]
                            };

                            io::stdout().write_all(&to_write).unwrap();
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
