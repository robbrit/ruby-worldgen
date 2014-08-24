require 'optparse'
require "worldgen"

options = {}

OptionParser.new do |opts|
  opts.on("--heightmap [FILE]", String, "Output a heightmap to FILE") do |file|
    options[:heightmap] = file
  end

  opts.on("--platemap [FILE]", String, "Output a platemap to FILE") do |file|
    options[:platemap] = file
  end

  opts.on("--size N", Integer, "Generate a map of size 2^N + 1") do |n|
    options[:size] = 2**n + 1
  end

  opts.on("--num-plates [N]", Integer, "Generate N plates") do |n|
    options[:num_plates] = n
  end
end.parse!

if not options[:size]
  puts "No size specified."
  exit
end

if options[:heightmap]
  puts "Generating heightmap..."
  heightmap = Worldgen::HeightMap.new(options[:size])
  Worldgen::Algorithms.diamond_square!(heightmap)
  Worldgen::Render.heightmap heightmap, options[:heightmap]
end

if options[:platemap]
  puts "Generating plate map..."
  if not options[:num_plates]
    puts "num_plates not specified."
    exit
  end

  platemap = Worldgen::PlateMap.new(options[:size])
  platemap.generate_plates! options[:num_plates]
  puts "Converting to height map..."
  heightmap = platemap.to_height_map
  render_heightmap heightmap, options[:platemap]
end

puts "Done."

