package com.ndk.myndk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.google.common.base.Function;
import com.google.common.base.Joiner;
import com.google.common.collect.Lists;
import com.google.common.primitives.Ints;
import com.ndk.myndk.exception.InvalidTypeException;
import com.ndk.myndk.exception.NotExistingKeyException;
import com.ndk.myndk.exception.StoreFullException;

import java.util.Arrays;
import java.util.List;
import java.util.regex.Pattern;

public class MainActivity extends AppCompatActivity implements StoreListener{
    //private Store mStore = new Store(this);
    private StoreThradeSafe mStore = new StoreThradeSafe(this);
    private long mWatcher;
    private EditText mUIKeyEdit, mUIValueEdit;
    private Spinner mUITypeSpiner;
    private Button mUIGetButton, mUISetButton;
    private Pattern mKeyPattern;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.showTitle);
        mKeyPattern = Pattern.compile("\\p{Alnum}");
        Log.d("mKeyPattern",mKeyPattern.toString());
        mUIKeyEdit = (EditText) findViewById(R.id.uiKeyEdit);
        mUIValueEdit = (EditText) findViewById(R.id.uiValueEdit);

        ArrayAdapter<StoreType> adapter = new ArrayAdapter<StoreType>(getApplicationContext(),
                R.layout.support_simple_spinner_dropdown_item,StoreType.values());
        adapter.setDropDownViewResource(
                R.layout.support_simple_spinner_dropdown_item);
        mUITypeSpiner = (Spinner) findViewById(R.id.uiTypeSpiner);
        mUITypeSpiner.setAdapter(adapter);

        mUIGetButton = (Button) findViewById(R.id.uiGetValueButton);
        mUIGetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                onGetValue();
            }
        });

        mUISetButton = (Button) findViewById(R.id.uiSetValueButton);
        mUISetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                onSetValue();
            }
        });




    }


    private void onGetValue() {
        String key = mUIKeyEdit.getText().toString();
        StoreType type = (StoreType) mUITypeSpiner.getSelectedItem();

      if(!mKeyPattern.matcher(key).matches()){
            displayMessage("Incorect key.");
            return;
        }

try {
    switch (type) {
        case IntegerArray:
            mUIValueEdit.setText(Ints.join(";", mStore.getIntegerArray(key)));
            break;
        case StringArray:
            mUIValueEdit.setText(Joiner.on(";").join(mStore.getStringArray(key)));
            break;
        case ColorArray:
            mUIValueEdit.setText(Joiner.on(";").join(mStore.getColorArray(key)));
            break;
        case Color:
            mUIValueEdit.setText(mStore.getColor(key).toString());
            break;
        case Boolean:
            mUIValueEdit.setText(Boolean.toString(mStore.getBoolean(key)));
            break;
        case Integer:
            mUIValueEdit.setText(Integer.toString(mStore.getInteger(key)));
            break;
        case String:
            mUIValueEdit.setText(mStore.getString(key));
            break;
        }
    } catch (NotExistingKeyException eNotExistingKeyException){
        displayMessage(eNotExistingKeyException.getMessage());
    }catch (InvalidTypeException eInvalidTypeException){
        displayMessage(eInvalidTypeException.getMessage());
    }
    }

    private void onSetValue() {
        String key = mUIKeyEdit.getText().toString();
        String value = mUIValueEdit.getText().toString();

        StoreType type = (StoreType) mUITypeSpiner.getSelectedItem();

        if (!mKeyPattern.matcher(key).matches()) {
            displayMessage("Incorect key.");
            return;
        }


       try {
            switch (type) {
                case IntegerArray:
                    mStore.setIntegerArray(key, Ints.toArray(stringToList(new Function<String, Integer>() {

                        @Override
                        public Integer apply(String input) {
                            return Integer.parseInt(input);
                        }
                    }, value)));
                    break;
                case StringArray:
                    String[] stringArray = value.split(";");
                    mStore.setStringArray(key, stringArray);
                    break;
                case ColorArray:
                    List<Color> idList = stringToList(new Function<String, Color>() {
                        @Override
                        public Color apply(String input) {
                            return new Color(input);
                        }
                    }, value);
                    mStore.setColorArray(key, idList.toArray(new Color[idList.size()]));
                    break;
                case Color:
                    mStore.setColor(key, new Color(value));
                    break;
                case Boolean:
                    mStore.setBoolean(key, Boolean.parseBoolean(value));
                    break;
                case Integer:
                    mStore.setInteger(key, Integer.parseInt(value));
                    break;
                case String:
                    mStore.setString(key, value);
                    break;

            }

        }catch(NumberFormatException eNumberFormatException){
            displayMessage(eNumberFormatException.getMessage());
        }
        catch(StoreFullException eStoreFullException){
            displayMessage(eStoreFullException.getMessage());
        }
        catch (Exception eException){
            displayMessage("Incorect key from exception.");
        }


        updateTitle();
    }

    private <TType>List<TType>stringToList(Function<String, TType> pConvesrion, String pValue) {
        String[] splitArray = pValue.split(";");
        List<String> splitList = Arrays.asList(splitArray);

        return Lists.transform(splitList,pConvesrion);
    }

    private void updateTitle() {
        int numEntries = mStore.getCount();
    //   getApplicationContext().setTitle(String.format("Store (%1$s)",numEntries));
    }

    private void displayMessage(String pMessage){
        Toast.makeText(getApplicationContext(),pMessage,Toast.LENGTH_LONG).show();
    }

    @Override
    public void onSuccess(int pValue) {
        displayMessage(String.format("Integer '%1$d' successfuly saved!",pValue));
    }

    @Override
    public void onSuccess(String pValue) {
        displayMessage(String.format("String '%1$s' successfuly saved!",pValue));
    }

    @Override
    public void onSuccess(Color pValue) {
        displayMessage(String.format("Color '%1$s' successfuly saved!",pValue));
    }

    @Override
    public void onResume(){
        super.onResume();
        mWatcher = mStore.startWatcher();
    }

    @Override
    public void onPause(){
        super.onPause();
        mStore.stopWatcher(mWatcher);
    }

}
