#!/usr/bin/python
import sys

number_of_processors = 4
processor_states = {}
cache_number = -1
has_data = 0

'''
Implements the logic of the automaton to decide what it will do.
Can return the following:
 X <- BusRdX
 S <- BusRd(S)
 # <- BusRd(S#)
 - <- BusRd/- or just -
 F <- BusRd/Flush*
'''
def active_processor_decision(processor_id, processor_action):
    if processor_states[processor_id] == 'I':
        if processor_action == 'w':
            return 'X'
        else:
            for state in processor_states:
                if processor_states[state]  == 'S' or processor_states[state] == 'E' or processor_states[state] == 'M':
                    return 'S'
            return '#'

    if processor_states[processor_id] == 'S':
        if processor_action == 'w':
            return 'X'
        else:
            if processor_states[processor_id] == 'S':
                return '-'
            for state in processor_states:
                if processor_states[state]  == 'S' or processor_states[state] == 'E' or processor_states[state] == 'M':
                    return 'F'
                
    if processor_states[processor_id] == 'E':
        return '-'

    if processor_states[processor_id] == 'M':
        return '-'

    return 'INVALID'

'''
Modifies the state of the active processor in the states dictionary
'''
def change_active_processor(processor_id, processor_action):
    global cache_number
    if processor_states[processor_id] == 'I':
        if processor_action == 'w':
            processor_states[processor_id] = 'M'
            return
        else:
            for state in processor_states:
                if processor_states[state]  == 'S' or processor_states[state] == 'E' or processor_states[state] == 'M':
                    processor_states[processor_id] = 'S'
                    cache_number = state
                    return
            processor_states[processor_id] = 'E'
            return

    if processor_states[processor_id] == 'S':
        if processor_action == 'w':
            processor_states[processor_id] = 'M'
        return
                
    if processor_states[processor_id] == 'E':
        return

    if processor_states[processor_id] == 'M':
        return

'''
Modifies the state of the other processors based on the chosen action
'''
def change_passive_processors(processor_id, processor_action):
    for i in range(len(processor_states)):
        if i == processor_id:
            continue
        if processor_states[i] == 'M':
            processor_states[i] = 'I' if processor_action == 'w' else 'S'
        if processor_states[i] == 'E':
            processor_states[i] = 'I' if processor_action == 'w' else 'S'
        if processor_states[i] == 'S':
            processor_states[i] = 'I' if processor_action == 'w' else 'S'
        if processor_states[i] == 'I':
            processor_states[i] = 'I' if processor_action == 'w' else 'I'

'''
Decodes the source from the chosen action of the automaton
'''
def parse_source(character):
    if character == '#':
        return 'Mem'
    if character == 'S':
        return 'Cache' + str(cache_number)
    if character == '-':
        return '-'
    if character == 'X':
        return 'Flush'
'''
Decodes the name of the action from it's contracted form
'''
def parse_action(character):
    global has_data
    if character == '#':
        has_data = 1
        return 'BusRd'
    if character == 'X':
        has_data = 0
        return 'BusRdX'
    if character == '-':
        return '-     '
    if character == 'S':
        return 'Flush' if has_data == 1 else 'BusRd'

'''
Prints the formatted output and calls the output for each step
'''
if __name__ == "__main__":
    states_string = ''
    for i in range(number_of_processors):
        processor_states[i] = 'I'
        states_string += processor_states[i] + ' '

    print('Argument format (index starts at 0): 0 r/w 1 r/w ... n-1 r/w\n')
    if len(sys.argv) % 2 == 0 or len(sys.argv) == 1:
        exit()
    print('t Act_Proc Stari Act_Mag Srs_date')
    print('t0 init    ' + states_string + ' -      -')

    for i in range(1, len(sys.argv), 2):
        processor_id = int(sys.argv[i])
        processor_action = sys.argv[i + 1]
        action = ''
        source = ''
        states_string = ''
        current_state = processor_states[processor_id]

        action = active_processor_decision(processor_id, processor_action)
        change_active_processor(processor_id, processor_action)
        change_passive_processors(processor_id, processor_action)

        for proc in range(number_of_processors):
            states_string += processor_states[proc] + ' '

        print('t' + str(int((i + 1) / 2)) + ' P' + str(processor_id) +
        ('Rd    ' if (processor_action == 'r') else 'Wr    ') + states_string +
        ' ' + parse_action(action) + ' ' + parse_source(action))


