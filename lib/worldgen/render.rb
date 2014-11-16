require 'RMagick'

module Worldgen::Render
  # Render a heightmap to a grayscale file.
  # Arguments:
  # * map - The heightmap to render
  # * filename - The filename to use. Image format will be inferred from the filename
  def self.heightmap map, filename
    # loading each one in is crazy slow, just throw it into a pixel map
    image = Magick::Image.new(map.size, map.size) { self.background_color = "black" }

    map.each_height do |x, y, pix_height|
      image.pixel_color x, y, grey(pix_height)
    end

    image.write filename
  end

  # Render a platemap to a file. Each plate has a separate colour - this will
  # only work for up to 16 plates, after that everything will be black.
  #
  # Arguments:
  # * map - the plate map to render
  # * filename - the filename to output to
  def self.platemap map, filename
    image = Magick::Image.new(map.size, map.size) { self.background_color = "black" }

    # draw plates
    colours = [
      "#FF0000", "#0000FF", "#FFFF00", "#00FF00",
      "#FF6600", "#FF00FF", "#00FFFF", "#CCCCCC",
      "#006600", "#000066", "#660066", "#666600",
      "#CCCCCC", "#FFFFFF", "#000000", "#CCCCFF"
    ]
    map.each_plate_point do |x, y, plate|
      begin
        image.pixel_color x, y, (colours[plate] or "#000000")
      rescue
        puts "colour fail"
        puts [x, y, plate, colours[plate]].inspect
      end
    end

    image.write filename
  end

  # Render a lattice to a file.
  #
  # Arguments:
  # * lattice - the latice to render
  # * filename - the file to output to
  # * width (optional) - the width of the image
  # * height (optional) - the height of the image
  def self.lattice lattice, filename, width=nil, height=nil
    width = width || lattice.width
    height = height || lattice.height

    image = Magick::Image.new(width, height) { self.background_color = "black" }

    stepx = lattice.width.fdiv(width)
    stepy = lattice.height.fdiv(height)
    
    lattice.each_point(0, 0, lattice.width, lattice.height, stepx, stepy) do |x, y, point|
      px = (x / stepx).floor
      py = (y / stepy).floor
      image.pixel_color px, py, grey(point)
    end

    image.write filename
  end

  # Render perlin noies
  #
  # Arguments:
  # * noise - the noise object to render
  # * filename - the filename to output to
  # * width - the width of the image
  # * height - the height of the image
  def self.perlin p, filename, width=256, height=256
    heightmap p.to_heightmap(width, height), filename
  end

private
  # Convert a number in [0, 1) to the grey hex code
  def self.grey float
    "##{("%2X" % (float * 256).floor) * 3}"
  end
end
