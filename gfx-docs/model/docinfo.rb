require 'json'

# Encapsulates the common information that may
# be described in a common Gfx doc-string.
class DocInfo
  # The name of the element described by the doc info.
  attr_accessor :name
  
  # The summary of the doc info.
  attr_accessor :abstract
  
  # The discussion of the doc info.
  attr_accessor :discussion
  
  # The associated links of the doc info,
  # an array of Link objects.
  attr_accessor :see_also
  
  # The type string of the doc info.
  def type
    DocInfoClasses.key(self.class)
  end
  
  
  # Returns the serializable attributes of the class. Overriden
  # by subclasses of DocInfo to provide implicit serialization.
  def self.serializable_attrs
    [:name, :abstract, :discussion, :see_also, :type]
  end
  
  # Converts the doc-info class into a hash.
  def to_hash(wants_nil_leaves: false)
    hash = {}
    self.class.serializable_attrs.each {|attr_sel|
      val = self.send attr_sel
      next unless val || wants_nil_leaves
      if val.is_a?(Array) && val[0].is_a?(DocInfo)
        val = val.map &:to_hash
      end
      hash[attr_sel] = val
    }
    hash
  end
end


# Encapsulates information about a module.
#
# ModuleDocInfo is unique in that it is the only
# DocInfo subtype that contains other DocInfo objects.
# Plain DocInfo objects are never emitted outside of
# a `module`, and it is currently impossible to nest
# modules.
class ModuleDocInfo < DocInfo
  # The child doc infos of the module.
  attr_accessor :doc_infos
  
  
  def initialize
    @doc_infos = []
  end
  
  # overrides
  def self.serializable_attrs
    super.concat [:doc_infos]
  end
end


# Encapsulates the information around function doc strings.
class FunctionDocInfo < DocInfo
  # The signature of the function. E.g. (<num> <num> -- <num>)
  attr_accessor :signature
  
  # The params of the function, an array of
  # strings describing each parameter.
  attr_accessor :params
  
  # The type that the function returns.
  attr_accessor :returns
  
  
  # overrides
  def self.serializable_attrs
    super.concat [:signature, :params, :returns]
  end
end


# Encapsulates information about a constant.
class ConstantDocInfo < DocInfo
  # The type of the constant.
  attr_accessor :type
  
  # overrides
  def self.serializable_attrs
    super.concat [:type]
  end
end


# Encapsulates the information around type doc strings.
class TypeDocInfo < DocInfo
  # The parent type of the type, a string.
  attr_accessor :supertype
  
  # The accessible fields of the type, if applicable.
  attr_accessor :fields
  
  # overrides
  def self.serializable_attrs
    super.concat [:supertype, :fields]
  end
end

# Maps the different doc info type names to their equivalent classes.
DocInfoClasses = {
  "module"    => ModuleDocInfo,
  "function"  => FunctionDocInfo,
  "constant"  => ConstantDocInfo,
  "type"      => TypeDocInfo,
}

# The directive names for all of the doc info types.
DocInfoDirectiveNames = DocInfoClasses.keys
