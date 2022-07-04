package com.odde.atddv2.ice;

import com.ice.server.DriverPrx;
import com.ice.server.DriverPrxHelper;
import com.ice.server.SerialPortPrx;
import com.ice.server.SerialPortPrxHelper;
import io.cucumber.java.After;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import io.cucumber.spring.CucumberContextConfiguration;
import org.mockserver.integration.ClientAndServer;
import org.mockserver.model.HttpResponse;
import org.springframework.boot.test.context.SpringBootContextLoader;
import org.springframework.test.context.ContextConfiguration;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;
import static org.mockserver.integration.ClientAndServer.startClientAndServer;
import static org.mockserver.model.HttpRequest.request;

@ContextConfiguration(classes = {CucumberConfiguration.class}, loader = SpringBootContextLoader.class)
@CucumberContextConfiguration
public class IceSteps {

    private String output;
    private Socket clientSocket;
    private BufferedReader in;
    private ClientAndServer mockServer;
    private String deviceInfo;

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

    @Given("device info {string}")
    public void device_info(String string) throws InterruptedException {
        mockServer = startClientAndServer(1080);
        mockServer.when(request().withMethod("GET").withPath("/device-info")).respond(HttpResponse.response()
                .withStatusCode(200)
                .withBody(string, StandardCharsets.UTF_8));
    }

    @When("ice get device info")
    public void ice_get_device_info() {
        try {
            Ice.Communicator ic = Ice.Util.initialize();
            Ice.ObjectPrx base = ic.stringToProxy("Driver:default -p 10000 -h localhost");
            DriverPrx driver = DriverPrxHelper.checkedCast(base);
            if (driver == null)
                throw new Error("Invalid proxy");
            deviceInfo = driver.readSerialPort();
            ic.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Then("device info is {string}")
    public void device_info_is(String string) {
        assertThat(deviceInfo).isEqualTo(string);
    }

    @After
    public void resetMockServer() {
        if (mockServer != null)
            mockServer.reset();
    }
}
