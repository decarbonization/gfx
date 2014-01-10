# Encapsulates the conversion of the internal documentation
# model into a human or machine readable external format.
class OutputWriter < Object
  # Takes an array of ModuleDocInfo objects and an output destination.
  # The output destination may either be a File, or a String representing
  # a location. It is the responsibility of the writer to handle both cases.
  #
  # Subclasses must override this method.
  def self.generate(modules, destination)
    throw NotImplementedError
  end
end

# Hash whose keys are human readable strings,
# and whose values are classes. Subclasses of
# OutputWriters should add themselves to
# this hash when loaded.
OutputWriters = {}
