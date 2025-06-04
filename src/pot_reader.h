#ifndef POT_READER_H
#define POT_READER_H

class PotReader {
  private:
    uint8_t _pin; // Pin number for the potentiometer
    int16_t _lastValue = -1;
  public:

    PotReader(uint8_t pin);
    virtual ~PotReader() = default;

    // Read the current value of the potentiometer
    int readValue();

};

#endif