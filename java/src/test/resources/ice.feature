Feature: Ice server serial port

  Scenario: stub serial port data
    Given ice server with serial port data "ice response"
    When ice client send request
    Then ice client get server response "ice response"

