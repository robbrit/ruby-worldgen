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

So far there is not much. You can do square heightmaps:

```ruby
# Create a 50x50 heightmap - this will be flat
heightmap = Worldgen::HeightMap.new 50
```

Then you can do a diamond square fractal to it:

```ruby
Worldgen::Algorithms.diamond_square! heightmap
```

Then dump it to an image file:

```ruby
Worldgen::Render.heightmap heightmap, "output.png"
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
