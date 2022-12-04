use std::path::Path;
use std::fs::{self, File};
use std::io::prelude::*;
use mp3_duration;

const path: &'static str = "./microsd backup";

fn main() {
    let mut output_str = String::from("uint16_t* file_lengths_ms[] = {\n"); // begin the output string containing a c-style 2D array

    let mp3_folders = fs::read_dir(&path).unwrap(); // get all subdirectories containing mp3 files
    for folder in mp3_folders { // go through each mp3 folder
        let folder_path = folder.unwrap().path();
        let mp3_files = fs::read_dir(folder_path).unwrap(); // get all mp3 files in the folder

        output_str.push_str("    (uint16_t[]){ ");

        for file in mp3_files { // go through each mp3 file
            let file_path = file.unwrap().path();
            let duration = mp3_duration::from_path(&file_path).unwrap(); // get duration of file
            let duration = duration.as_millis(); // convert to duration in milliseconds

            let duration_as_str = format!("{}, ", duration); // get the duration as a string and add a comment after it
            output_str.push_str(&duration_as_str);
        }

        output_str.push_str("},\n");
    }

    output_str.push_str("};"); // add ending

    let mut output_file = File::create("output.h").unwrap();
    output_file.write_all(output_str.as_bytes()).unwrap();
}
