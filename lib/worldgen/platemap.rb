module Worldgen
  # A map that generates plates within a square map.
  class PlateMap
    # A class representing a single plate.
    # TODO: using Ruby for this is kinda slow, maybe port a good chunk of this
    # stuff to C
    class Plate
      # Attributes:
      # * seed - the initial point within the map for this plate
      # * map - the map that this plate is in
      # * id - the ID number of this plate (how this plate identifies itself)
      # * type - the type of plate (continental vs. ocean)
      attr_accessor :seed, :map, :id, :type

      # Set the seed for this plate. Construct the frontier.
      def seed= seed
        @seed = seed
        @frontier = empty_neighbours(@seed)
      end

      # See if this plate has a frontier or not
      def has_frontier?
        @frontier and @frontier.length > 0
      end

      # Get the length of this plate's frontier
      def frontier_length
        @frontier ? @frontier.length : 0
      end

      # Absorb a single empty point along the frontier
      def absorb_frontier!
        # Shuffle the frontier so that we end up grabbing a random point
        @frontier.shuffle!
        value = nil

        # It's possible another plate has absorbed part of our frontier since
        # the last time we set our frontier, so shift until we find an empty
        # spot
        while value == nil and @frontier.length > 0
          value = @frontier.shift
          value = nil unless at(*value) < 0
        end

        if value
          # move it into me
          @map[value[0]][value[1]] = @id

          # add new points onto my frontier
          @frontier += empty_neighbours(value)
        end

        value
      end

      # Get the empty neighbours around `point`
      def empty_neighbours point
        neighbours(point).select do |(x, y)|
          at(x, y) < 0
        end
      end

      # Get the neighbours of `point` - directly adjacent only, no diagonals
      def neighbours point
        [
          [-1, 0],
          [1, 0],
          [0, 1],
          [0, -1]
        ].map do |(dx, dy)|
          [
            (point[0] + dx + @map.length) % @map.length,
            (point[1] + dy + @map.length) % @map.length
          ]
        end
      end

    private
      # Get the point at x, y
      def at x, y
        @map[x][y]
      end
    end

    attr_reader :size

    def initialize size
      @size = size
      @plate_ids = nil
      @plates = nil
    end

    # Get the number of points in the map.
    def num_points
      @size * @size
    end

    # Convert to a height map - very simple, just give the continents one height
    # and the oceans another height. This is controlled by a single parameter,
    # (({sea_gap})), which will be the difference between the continents height and
    # oceans height.
    def to_height_map sea_gap
      raise "Sea gap should be between 0 and 1" if sea_gap < 0 or sea_gap > 1

      plate_heights = @plates.map do |plate|
        if plate.type == :continent
          0.5 + sea_gap / 2
        else
          0.5 - sea_gap / 2
        end
      end

      map = HeightMap.new @size
      
      each_plate_point do |x, y, id|
        map[x, y] = plate_heights[id]
      end

      map
    end

    # Iterate across the entire map.
    #
    # Usage:
    #
    #   each_plate_point do |x, y, plate_id|
    #     # do something with this information
    #   end
    def each_plate_point
      (0...@size).each do |x|
        (0...@size).each do |y|
          yield x, y, @plate_ids[x][y]
        end
      end
    end

    # Generate plates within this map.
    #
    # Arguments:
    # * num_plates - the number of plates to generate
    #
    # Options:
    # * verbose (default: true) - output logging while we're generating
    def generate_plates! num_plates, options={}
      verbose = options[:verbose] || true

      @plate_ids = Array.new(@size) { Array.new(@size) { -1 }}

      # Initialize plates in random spots
      @plates = (0...num_plates).map do |plate_num|
        # Find an unoccupied point
        point = nil
        while point == nil
          x, y = rand(@size), rand(@size)

          point = [x, y] if @plate_ids[x][y] < 0
        end

        x, y = point
        @plate_ids[x][y] = plate_num

        Plate.new.tap do |plate|
          plate.map = @plate_ids
          plate.id = plate_num
          plate.type = rand < 0.5 ? :continent : :ocean
          plate.seed = point
        end
      end

      num_points = self.num_points - num_plates
      valid_plates = @plates.select(&:has_frontier?)

      i = 0
      while valid_plates.length > 0
        if verbose and i % (num_points / 100) == 0
          puts "#{i}/#{num_points} #{(i.fdiv(num_points) * 100).round}%"
        end

        # Find a plate with neighbours
        loop do
          idx = choose_plate valid_plates
          plate = valid_plates[idx]

          # absorb a point from the frontier
          value = plate.absorb_frontier!

          if not value
            valid_plates.delete_at idx
            break if valid_plates.length == 0
          else
            # Did we just consume the last point of this plate?
            valid_plates.delete_at(idx) if not plate.has_frontier?
            break
          end
        end

        i += 1
      end
    end
    
    private

    # Choose a plate randomly from a list of plates
    def choose_plate plates
      # Weighted choice based on frontier length - if we do it perfectly uniform
      # then we end up with plates that "compress" into small remaining places
      # which results in "snaky" plates and weird convergence points between
      # many plates
      
      # TODO: could probably pull weighted randoms into a different module
      total = plates.map(&:frontier_length).inject(&:+)

      # TODO: using a uniform random generator here gives plates that are all
      # roughly the same size - kinda boring, maybe try using a non-uniform
      # distribution here
      point = rand(total)

      idx = 0
      begin
        while point > plates[idx].frontier_length
          point -= plates[idx].frontier_length
          idx += 1
        end
      rescue
        # TODO: fix this - once in a while we get an out of bounds problem here
        puts $!
        puts $!.backtrace.join("\n")
        puts "Point: #{point}"
        puts "Idx: #{idx}"
        puts "Total: #{total}"
      end

      idx
    end
  end
end
