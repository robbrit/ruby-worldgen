module Worldgen
  class HeightMap
    attr_reader :size

    def initialize size
      @size = size
      ObjectSpace.define_finalizer(self, method(:finalize))
      initialize_native(size)
    end

    def finalize
      finalize_native
    end
  end
end
