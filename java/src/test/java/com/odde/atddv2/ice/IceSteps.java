package com.odde.atddv2.ice;

import com.ice.server.SerialPortPrx;
import com.ice.server.SerialPortPrxHelper;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import io.cucumber.spring.CucumberContextConfiguration;
import org.springframework.boot.test.context.SpringBootContextLoader;
import org.springframework.test.context.ContextConfiguration;

@ContextConfiguration(classes = {CucumberConfiguration.class}, loader = SpringBootContextLoader.class)
@CucumberContextConfiguration
public class IceSteps {

    @Given("ice server with serial port data {string}")
    public void ice_server_with_serial_port_data(String string) {
    }

    @When("ice client send request")
    public void ice_client_send_request() {
        try {
            Ice.Communicator ic = Ice.Util.initialize();
            Ice.ObjectPrx base = ic.stringToProxy("SerialPort:default -p 10000 -h localhost");
            SerialPortPrx serialPort = SerialPortPrxHelper.checkedCast(base);
            if (serialPort == null)
                throw new Error("Invalid proxy");
            String output = serialPort.readSerialPort();
            System.out.println("output = " + output);
            ic.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Then("ice client get server response {string}")
    public void ice_client_get_server_response(String string) {
    }
}
