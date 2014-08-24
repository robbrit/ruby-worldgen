# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'worldgen/version'

Gem::Specification.new do |spec|
  spec.name          = "worldgen"
  spec.version       = Worldgen::VERSION
  spec.authors       = ["Rob Britton"]
  spec.email         = ["rob@robbritton.com"]
  spec.summary       = %q{Gem to handle procedural content generation for worlds.}
  #spec.description   = %q{TODO: Write a longer description. Optional.}
  spec.homepage      = ""
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0")
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.extensions << "ext/worldgen/extconf.rb"

  spec.add_dependency "rmagick", "~> 2.13.3"
  spec.add_development_dependency "bundler", "~> 1.6"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "rake-compiler", "~> 0.9.3"
end
