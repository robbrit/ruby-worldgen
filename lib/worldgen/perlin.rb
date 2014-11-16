require 'matrix'

module Worldgen
  class Perlin
    NUM_PERMUTATIONS = 0x100
    OFFSET = 0x1000
    PERMUTE_MASK = 0xFF
    SCALE = 0.33

    def initialize width, height
      @permutations = (0...NUM_PERMUTATIONS).to_a.shuffle
      @gradients = (0..NUM_PERMUTATIONS).map do
        Vector[rand - 0.5, rand - 0.5].normalize
      end

      # Extend the permutations and gradients to add more randomness
      @permutations = @permutations * 2 + @permutations[0, 2]
      @gradients = @gradients * 2 + @gradients[0, 2]
    end

    # Get the value at x, y
    # Algorithm taken from Ken Perlin's homepage:
    # http://mrl.nyu.edu/~perlin/doc/oscar.html
    def value_at x, y
      x += OFFSET
      y += OFFSET

      bx0, by0 = x.floor & PERMUTE_MASK, y.floor & PERMUTE_MASK
      bx1, by1 = (bx0 + 1) & PERMUTE_MASK, (by0 + 1) & PERMUTE_MASK

      rx0, ry0 = x - x.floor, y - y.floor
      rx1, ry1 = rx0 - 1.0, ry0 - 1.0

      sx, sy = _interp(rx0), _interp(ry0)

      # Choose some indexes from our permutation index list
      i = @permutations[bx0]
      j = @permutations[bx1]

      idx00 = @permutations[i + by0]
      idx01 = @permutations[i + by1]
      idx10 = @permutations[j + by0]
      idx11 = @permutations[j + by1]

      # Now grab some gradients based on those indexes
      g00 = @gradients[idx00]
      g01 = @gradients[idx01]
      g10 = @gradients[idx10]
      g11 = @gradients[idx11]

      # Calculate dot-products between our point and the corners
      u = Vector[rx0, ry0].inner_product(g00)
      v = Vector[rx1, ry0].inner_product(g10)
      # Interpolate between those dot-products
      a = lerp(u, v, sx)

      # Repeat on the right-side
      u = Vector[rx0, ry1].inner_product(g01)
      v = Vector[rx1, ry1].inner_product(g11)
      b = lerp(u, v, sx)

      # And interpolate one last time
      lerp(a, b, sy)
    end

    def _interp x
      x * x * (3.0 - 2.0 * x)
    end

    def lerp a, b, x
      a + x * (b - a)
    end

    def to_heightmap width, height
      hm = HeightMap.new(width)

      (0...width).each do |x|
        (0...width).each do |y|
          hm[x, y] = value_at(x * SCALE, y * SCALE)
        end
      end

      hm.normalize!(0.3, 1.0)
      hm
    end
  end
end
