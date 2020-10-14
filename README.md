# Obligatory Input Simulator
A small helper tool to solve the obligatories more efficiently.
It will automatically inject keystrokes based on config.json so you don't have to manually type in all the data/commands every time you rerun your application it.
Hopefully it will speed up the debugging process a bit :)

## Configuration
All the input configuration is based on virtual key codes which you can find [here](http://cherrytree.at/misc/vk.htm)

| Setting | What |
|---|---|
|activation_key | Key used to make it start injecting input into the active window|
|exit_key | Key used to exit the application|
|reload_config_key | Key used to reload the current config file|
|default_delay | If you don't specify a delay in the command, then this value will be used|
|automatic_newline | If this is set to true, it will automatically add a new line with each command (you don't need to add \n at the end)|


#### Explaining the Default Config Settings
##### settings
`"activation_key": 121` sets the activation key to F10.

`"exit_key": 123` sets the exit key to F12.

`"reload_config_key": 120` sets the exit key to F9.

`"default_delay": 10` sets the default delay to 10 milliseconds.

`"automatic_newline": false` disables automatic newlines when the command is finished.

##### commands
`["L\n"]` will inject the single character 'L' and a newline (enter key) after it.

`["L"]` will inject the single character 'L'.

`["L", 100]` will first wait 100 milliseconds and then inject the single character 'L'.

`["The Lord of the Rings: The Return of the King\n", 10]` will wait 10 milliseconds, then inject the string "The Lord of the Rings: The Return of the King" and then inject a newline.

## Usage
In order to use it you just have to put config.json within the same folder as the executable and then run it.

## Future
I will update this with the new obligatories when they are released.
And I will also add new features if they help improve the tool in any way, so let me know.
