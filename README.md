## Pebble Smartwatch SDK Examples

This repository contains the examples that are shipped with the Pebble
Smartwatch SDK to make it easy for you to fork them.

For SDK download and installation instructions, see:

 * <http://developer.getpebble.com/>


#### Description of directory content

Content overview:

 * `watches` -- Complete watch face demos.

 * `demos` -- Includes simple "feature" demos that show how to use
              specific features (e.g timers, vibes, custom fonts) and
              more complex "app" demos.

 * `templates` -- A couple of minimal apps that serve as a good
                  starting point for your own apps.


### Building the examples

The examples are designed to have the files from the SDK symlinked
into each directory and then the code compiled with:

    ./waf configure build

For example, to build the `Classio` watch:

    cd watches

    /<sdk_path>/tools/create_pebble_project.py --symlink-only /<sdk_path>/sdk/ classio

Then follow the instructions given by the script.


### Creating a new project

To create a new project from scratch with the same script:

     /<sdk_path>/tools/create_pebble_project.py /<sdk_path>/sdk/ new_project_name

Then change into the project directory and run:

    ./waf configure build

