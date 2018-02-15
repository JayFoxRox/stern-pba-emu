This has been hacked together in a rush.
It shows how to use the Stern emulation libraries from FarSights Pinball Arcade games.

It has only been tested with MinGW, SternGB.dll, 5409-0.0001.

# Usage

Make sure you are using a 32-bit compiler, then do the standard CMake build process:

```
mkdir build
cd build
cmake ..
make
```

Use the [pba-tools](https://github.com/JayFoxRox/pba-tools) rez utility to extract the ROM file (also image.bin / Flash in Stern terminology) from the games REZ file.
Then copy all required DLLs to your stern-pba-emu folder and run the following:

```
./stern-pba-emu SternGB.dll 5409-0.0001
```

Make sure to have all DLL dependencies in your build folder. In case of SternGB.dll (at the time of writing) these are msvcp110d.dll and msvcr110d.dll.

# Keyboard bindings

* <kbd>F5</kbd> - <kbd>F6</kbd>: Coin door buttons
* <kbd>Left-Ctrl</kbd>: Left flipper
* <kbd>Right-Ctrl</kbd>: Right flipper
* <kbd>Enter</kbd>: Start button

# Screenshots

![Logo (Top)](https://i.imgur.com/Zp1Xv8I.png)
![Logo (Bottom)](https://i.imgur.com/FGLctcq.png)
![Gozer Champion](https://i.imgur.com/3MWy8g3.png)
![Skillshot](https://i.imgur.com/mO4MLgm.png)
![Menu](https://i.imgur.com/FxUnybF.png)

---

If you like my work, a donation would be nice:

* [Patreon](https://www.patreon.com/jayfoxrox)
* [PayPal](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=x1f3o3x7x%40googlemail%2ecom&lc=GB&item_name=Jannik%20Vogel%20%28JayFoxRox%29&no_note=0&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHostedGuest)


(C) 2018 Jannik Vogel (JayFoxRox)

Released under GPLv3 license.
See license.txt for more information.
Create an issue if you need another license.

The license is strictly for the code in this repository, it does not apply to other copyright holders work, such as DLL or ROM files.
Redistribution with those binaries would be a violation of the GPL.

This project and its author are not affiliated with Stern Pinball or FarSight Studios in any way or form.
Any files and trademarks belong to their respective owners.
