# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'immutable_list/version'

Gem::Specification.new do |spec|
  spec.name          = "immutable_list"
  spec.version       = ImmutableList::VERSION
  spec.authors       = ["gam0022"]
  spec.email         = ["gam0022@gmail.com"]
  spec.description   = %q{Immutable Linked List implemented in C-Extensions}
  spec.summary       = %q{Immutable Linked List implemented in C-Extensions}
  spec.homepage      = ""
  spec.license       = "MIT"
  spec.extensions    = %w[ext/immutable_list/extconf.rb]

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
end
