# ImmutableList

Immutable Linked List of Ruby implemented in C-Extensions

## Installation

Add this line to your application's Gemfile:

    gem 'immutable_list'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install immutable_list

## Usage

### Basic

```
require 'immutable_list'

p ImmutableList.new # => ()

p l1 = ImmutableList.new.cons(1).cons(2).cons(3) # => (3, 2, 1)
p l1.head # => 3
p l1.tail # => (2, 1)

p l2 = ImmutableList[1, 2, 3, "a", "b"] #=> (1, 2, 3, "a", "b")

p l1.rev_append(l2) #=> (1, 2, 3, 1, 2, 3, "a", "b")
p l1.rev #=> (1, 2, 3)
p l3 = l1.append(l2) #=> (3, 2, 1, 1, 2, 3, "a", "b")

p l3.length # => 8
p ImmutableList[].length #=> 0

p l3.nth(0) #=> 3
p l3[7] #=> "b"
p l3[100] #=> nil
```

### QuickSort

A Example of QuickSort.

```
require 'immutable_list'

def divide(a, l, lt, ge)
  if l.empty?
    [lt, ge]
  elsif l.head < a
    divide(a, l.tail, lt.cons(l.head), ge)
  else
    divide(a, l.tail, lt, ge.cons(l.head))
  end
end

def qsort(l)
  if l.empty?
    ImmutableList.new
  else 
    lt, ge = divide(l.head, l.tail, ImmutableList.new, ImmutableList.new)
    qsort(lt) + qsort(ge).cons(l.head)
  end
end

l = ImmutableList[3, 5, 8, 1, 4, 7, 10, -3, 2, 100, 43, 10, 50]
p qsort(l) #=> (-3, 1, 2, 3, 4, 5, 7, 8, 10, 10, 43, 50, 100)
```


## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
