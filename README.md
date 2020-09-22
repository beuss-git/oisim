# Obligatory Input Simulator

## Configuration
All the input configuration is based on virtual key codes which you can find [here](http://cherrytree.at/misc/vk.htm)

| Setting | What |
|---|---|
|activation_key | Key used to make it start injecting input into the active window|
|exit_key | Key used to exit the application|
|default_delay | If you don't specify a delay in the command, then this value will be used|
|automatic_newline | If this is set to true, it will automatically add a new line with each command (you don't need to add \n at the end)|


## Explaining the Default Config Settings
##### settings
`"activation_key": 121` sets the activation key to 121 (F10)

`"exit_key": 123` sets the exit key to 123 (F12)

`"default_delay": 10` sets the default delay to 10 milliseconds

`"automatic_newline": false` disables automatic newlines when the command is finished

##### commands
`["L\n"]` will inject the single character 'L' and a newline (enter key) after it.

`["L"], ["L"]` will inject two 'L' characters on the same line

`["L", 100]` will first wait 100 milliseconds, then inject the single character 'L'

`["The Lord of the Rings: The Return of the King\n", 10]` will wait 10 milliseconds, then inject the string "The Lord of the Rings: The Return of the King" and then inject a newline.

## Usage
In order to use it you just have to put config.json within the same folder as the executable and then run the executable.







