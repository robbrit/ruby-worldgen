# Worldgen

Worldgen allows you to generate random worlds.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'worldgen'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install worldgen

## Usage

### Heightmaps

```ruby
# Create a 50x50 heightmap - this will be flat
heightmap = Worldgen::HeightMap.new 50
```

Then dump it to an image file:

```ruby
Worldgen::Render.heightmap heightmap, "output.png"
```

### Plate Maps

A plate map is a random construction of plates within a world (as in plate
tectonics). Example:

```ruby
# create a 256x256 plate map
platemap = Worldgen::PlateMap.new 256
# generate 10 plates
platemap.generate_plates! 10
# Output to a PNG - this will show each plate in a different colour
Worldgen::Render.platemap platemap, "plates.png"
```

### Perlin Noise

This feature is experimental and may be subject to change in the near future.

```ruby
# Create the noise object
noise = Worldgen::Perlin.new(256, 256)
# Draw it
Worldgen::Render.perlin noise, "noise.png"
```

## Contributing

1. Fork it ( https://github.com/robbrit/worldgen/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

### Notes

Heightmaps are purely written in C for performance. If you're going to be
writing code that interacts with heightmaps, you're going to have a much
better time doing it in C since Ruby is a fair bit too slow once you get to
larger maps (512x512 or higher).

### TODO

* Diamond Square output seems to be rougher than it should be
