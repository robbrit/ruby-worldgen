require 'RMagick'

module Worldgen::Render
  def self.heightmap map, filename
    # loading each one in is crazy slow, just throw it into a pixel map
    image = Magick::Image.new(map.size, map.size) { self.background_color = "black" }

    map.each_height do |x, y, pix_height|
      grey = ("%2X" % (pix_height * 255).round) * 3
      image.pixel_color x, y, "##{grey}"
    end

    #image.display
    image.write filename
  end

  def self.platemap map, filename
    image = Magick::Image.new(map.size, map.size) { self.background_color = "black" }

    # draw plates
    colours = [
      "#FF0000", "#0000FF", "#FFFF00", "#00FF00",
      "#FF6600", "#FF00FF", "#00FFFF", "#CCCCCC",
      "#006600", "#000066", "#660066", "#666600"
    ]
    map.each_plate_point do |x, y, plate|
      begin
        image.pixel_color x, y, colours[plate]
      rescue
        puts "colour fail"
        puts [x, y, plate, colour[plate]].inspect
      end
    end

    #image.display
    image.write filename
  end
end
