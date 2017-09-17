import os
import virtualbox
import json

if __name__ == '__main__':
    config = json.load(open('config.json'))
    machine = config['start_config']['machine']
    start_type = config['start_config']['type']
    # Starting VM
