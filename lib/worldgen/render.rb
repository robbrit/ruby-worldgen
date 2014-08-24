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
      grey = ("%2X" % (pix_height * 255).round) * 3
      image.pixel_color x, y, "##{grey}"
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
end
