module Worldgen
  # A square heightmap
  class HeightMap
    # A class used internally to manage C-allocated memory
    class HeightmapData
    end

    attr_reader :size

    # Create a new square heightmap.
    # Arguments:
    # * size - the width/height of the map
    def initialize size
      @size = size
      initialize_native(size)
    end
  end
end
