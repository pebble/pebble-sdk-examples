## Pebble Smartwatch SDK.

Welcome!

For installation instructions and more, see: <http://developer.getpebble.com/>


#### Description of directory content

Content overview:

 * `watches` -- Complete watch face demos.

 * `demos` -- Includes simple "feature" demos that show how to use
              specific features (e.g timers, vibes, custom fonts) and
              more complex "app" demos.

 * `templates` -- A couple of minimal apps that serve as a good
                  starting point for your own apps.

 * `tools` --  As used in section above.


### Building the examples

The examples are designed to have the files from the SDK symlinked
into each directory and then the code compiled with:

    ./waf configure build

For example, to build the `Classio` watch:

    cd watches

    ../tools/create_pebble_project.py --symlink-only ../sdk/ classio

Then follow the instructions given by the script.


### Creating a new project

To create a new project from scratch with the same script:

     ../tools/create_pebble_project.py ../sdk/ new_project_name

Then change into the project directory and run:

    ./waf configure build

