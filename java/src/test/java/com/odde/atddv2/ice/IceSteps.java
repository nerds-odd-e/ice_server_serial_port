package com.odde.atddv2.ice;

import com.ice.server.SerialPortPrx;
import com.ice.server.SerialPortPrxHelper;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import io.cucumber.spring.CucumberContextConfiguration;
import org.springframework.boot.test.context.SpringBootContextLoader;
import org.springframework.test.context.ContextConfiguration;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

import static org.assertj.core.api.Assertions.assertThat;

@ContextConfiguration(classes = {CucumberConfiguration.class}, loader = SpringBootContextLoader.class)
@CucumberContextConfiguration
public class IceSteps {

    private String output;
    private Socket clientSocket;
    private BufferedReader in;

    @Given("ice server with serial port data {string}")
    public void ice_server_with_serial_port_data(String response) throws IOException, InterruptedException {
        Runtime.getRuntime().exec(new String[]{"bash", "-c", String.format("echo -en \"%s\\0\" | nc localhost 4641", response)}).waitFor();
    }

    @When("ice client send request")
    public void ice_client_send_request() {
        try {
            Ice.Communicator ic = Ice.Util.initialize();
            Ice.ObjectPrx base = ic.stringToProxy("SerialPort:default -p 10000 -h localhost");
            SerialPortPrx serialPort = SerialPortPrxHelper.checkedCast(base);
            if (serialPort == null)
                throw new Error("Invalid proxy");
            output = serialPort.readSerialPort();
            ic.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Then("ice client get server response {string}")
    public void ice_client_get_server_response(String expected) {
        assertThat(expected).isEqualTo(output);
    }

    @When("ice client send write request {string}")
    public void iceClientSendWriteRequest(String message) {
        try {
            Ice.Communicator ic = Ice.Util.initialize();
            Ice.ObjectPrx base = ic.stringToProxy("SerialPort:default -p 10000 -h localhost");
            SerialPortPrx serialPort = SerialPortPrxHelper.checkedCast(base);
            if (serialPort == null)
                throw new Error("Invalid proxy");
            serialPort.writeSerialPort(String.format("%s\n", message));
            ic.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Given("connect to serial port")
    public void connectToSerialPort() throws IOException {
        clientSocket = new Socket("localhost", 4641);
        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
    }

    @Then("serial port write {string}")
    public void serialPortWrite(String expected) throws IOException {
        assertThat(in.readLine()).isEqualTo(expected);
    }
}
