Gem::Specification.new do |s|
  s.name        = 'sorbet-runtime'
  s.version     = '0.0.0'
  s.summary     = 'Sorbet runtime'
  s.description = "Sorbet's runtime type checking component"
  s.authors     = ['Stripe']
  s.files       = Dir.glob('lib/**/*')
  s.homepage    = 'https://sorbet.run'
  s.license     = 'Apache-2.0'
  s.metadata = {
    "source_code_uri" => "https://github.com/sorbet/sorbet"
  }

  s.required_ruby_version = ['>= 2.3.0', '< 2.7.0.preview1']

  s.add_development_dependency 'minitest', '~> 5.11'
  s.add_development_dependency 'mocha', '~> 1.7'
  s.add_development_dependency 'rake'
  # for reproducing race conditions in tests
  s.add_development_dependency 'concurrent-ruby', '~> 1.1.5'
end
