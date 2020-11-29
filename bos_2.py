import pandas as pd

def main():
    input_file = open('output.txt', 'r')
    place = 0
    writer = pd.ExcelWriter('bos_2.xlsx', engine='xlsxwriter')
    # Считываем каждую строку
    while 1:
        line = input_file.readline()
        # Если нет строки значит все обработали
        if line == "":
            break
        # Проходим по всей строке обрезая по пробелам
        part = 0
        timestamp = 0
        event_type = ""
        urb_type_dir = ""
        bus_number = ""
        device_address = ""
        endpoint_number = ""
        data_length = ""
        data_words = ""
        saved_s = ""
        s = ""
        i = 0
        line_len = int(len(line))
        for i in range(line_len):
            if (line[i] != " " and line[i] != "\n"):
                    s += line[i]
            else:
                # URB Tag.
                if (part == 0):
                    s = ""
                    part += 1
                # Timestamp in microseconds, a decimal number.
                elif (part == 1):
                    timestamp = int(s)
                    s = ""
                    part += 1
                # Event Type.
                elif (part == 2):
                    if (s == "S"):
                        event_type = "submission"
                    elif (s == "C"):
                        event_type = "callback"
                    elif (s == "E"):
                        event_type = "submission error"
                    s = ""
                    part += 1
                # "Address" word.
                elif (part == 3):
                    # URB type and direction.
                    if (s[0:2] == "Ci"):
                        urb_type_dir = "Control input"
                    elif (s[0:2] == "Co"):
                        urb_type_dir = "Control output"
                    elif (s[0:2] == "Zi"):
                        urb_type_dir = "Isochronous input"
                    elif (s[0:2] == "Zo"):
                        urb_type_dir = "Isochronous output"
                    elif (s[0:2] == "Ii"):
                        urb_type_dir = "Interrupt input"
                    elif (s[0:2] == "Io"):
                        urb_type_dir = "Interrupt output"
                    elif (s[0:2] == "Bi"):
                        urb_type_dir = "Bulk input"
                    elif (s[0:2] == "Bo"):
                        urb_type_dir = "Bulk output"
                    # Bus number.
                    bus_number = s[3]
                    # Device address.
                    device_address += s[5:8]
                    # Endpoint number.
                    endpoint_number = s[9]
                    s = ""
                    part += 1
                # Data Length.
                elif (part == 4):
                    if (s == ">"):
                        data_length = saved_s
                        part += 1
                    elif (s == "<"):
                        data_length = saved_s
                        part += 1
                    elif (s == "="):
                        data_length = saved_s
                        part += 1
                    saved_s = s
                    s = ""
                # Data words.
                elif (part == 5):
                    data_words += s
                    s = ""
        # Запись информации в файл
        packet = pd.DataFrame(
            {"Timestamp" : [timestamp],
            "Event Type" : [event_type],
            "URB type and direction" : [urb_type_dir],
            "Bus number" : [bus_number],
            "Device address" : [device_address],
            "Endpoint number" : [endpoint_number],
            "Data Length" : [data_length],
            "Data words" : [data_words]
            })

        if place == 0:
            packet.to_excel(writer, startrow=place, index = 0)
            place += 2
        else:
            packet.to_excel(writer, startrow=place, header = 0, index = 0)
            place += 1
    writer.save()

main()