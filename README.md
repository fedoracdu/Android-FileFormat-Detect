# Android-FileFormat-Detect
Detect file's format information based on file's content, not extension name

this module needs NDK to compile

Usage:
1. call getInstance.init(context) to finish initiation, you can canll setListener to receive the notification
2. after the initiation, call getFileFormat(path) to get the file's formation information
